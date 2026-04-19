import { Redis } from '@upstash/redis';

// Initialize Redis if credentials exist, otherwise fallback to memory
const redis = (process.env.UPSTASH_REDIS_REST_URL && process.env.UPSTASH_REDIS_REST_TOKEN)
  ? new Redis({
      url: process.env.UPSTASH_REDIS_REST_URL,
      token: process.env.UPSTASH_REDIS_REST_TOKEN,
    })
  : null;

const ANIMAL_TIMEOUT = 10000; // 10 seconds

// Memory fallback for local development or missing Redis
let memoryStore = {};
let memoryTimestamps = {};

export const Storage = {
  async set(animal, zone) {
    if (redis) {
      await redis.hset('animals', { [animal]: zone });
      await redis.hset('timestamps', { [animal]: Date.now() });
    } else {
      memoryStore[animal] = zone;
      memoryTimestamps[animal] = Date.now();
    }
  },

  async getAll() {
    const now = Date.now();
    let stores = {};
    let timestamps = {};

    if (redis) {
      try {
        stores = await redis.hgetall('animals') || {};
        timestamps = await redis.hgetall('timestamps') || {};
      } catch (err) {
        console.error('Redis error fallback to memory:', err);
        stores = { ...memoryStore };
        timestamps = { ...memoryTimestamps };
      }
    } else {
      stores = { ...memoryStore };
      timestamps = { ...memoryTimestamps };
    }

    const active = {};
    const expired = [];

    for (const [animal, zone] of Object.entries(stores)) {
      const ts = parseInt(timestamps[animal]) || 0;
      if (now - ts > ANIMAL_TIMEOUT) {
        expired.push(animal);
      } else {
        active[animal] = zone;
      }
    }

    // Cleanup expired in background
    if (expired.length > 0) {
      try {
        if (redis) {
          await redis.hdel('animals', ...expired);
          await redis.hdel('timestamps', ...expired);
        } else {
          expired.forEach(a => {
            delete memoryStore[a];
            delete memoryTimestamps[a];
          });
        }
      } catch (e) {}
    }

    return active;
  }
};
