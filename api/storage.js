// Shared data storage for all API routes
// Timeout: Clear animals not updated within 10 seconds

let dataStore = {};
let updateTimestamps = {};

const ANIMAL_TIMEOUT = 10000; // 10 seconds (animals send every 3 seconds)

const Storage = {
  // Set animal data
  set: (animal, zone) => {
    dataStore[animal] = zone;
    updateTimestamps[animal] = Date.now();
  },

  // Get all animal data (only active ones)
  getAll: () => {
    Storage.cleanupStaleAnimals();
    return { ...dataStore };
  },

  // Get single animal
  get: (animal) => {
    Storage.cleanupStaleAnimals();
    return dataStore[animal] || null;
  },

  // Remove animals not updated recently
  cleanupStaleAnimals: () => {
    const now = Date.now();
    const staleAnimals = [];

    for (const animal in updateTimestamps) {
      if (now - updateTimestamps[animal] > ANIMAL_TIMEOUT) {
        staleAnimals.push(animal);
      }
    }

    staleAnimals.forEach(animal => {
      delete dataStore[animal];
      delete updateTimestamps[animal];
      console.log(`🗑️ Removed stale animal: ${animal}`);
    });

    return staleAnimals;
  },

  // Get with timestamps
  getAllWithTimestamps: () => {
    Storage.cleanupStaleAnimals();
    const result = {};
    for (const animal in dataStore) {
      result[animal] = {
        zone: dataStore[animal],
        lastUpdated: new Date(updateTimestamps[animal]).toISOString()
      };
    }
    return result;
  },

  // Clear all data
  clear: () => {
    dataStore = {};
    updateTimestamps = {};
    console.log('🗑️ All animal data cleared');
  },

  // Delete specific animal
  delete: (animal) => {
    delete dataStore[animal];
    delete updateTimestamps[animal];
    console.log(`🗑️ Deleted animal: ${animal}`);
  },

  // Check if animal exists
  has: (animal) => {
    Storage.cleanupStaleAnimals();
    return animal in dataStore;
  },

  // Get count
  count: () => {
    Storage.cleanupStaleAnimals();
    return Object.keys(dataStore).length;
  },

  // Get all stale animals (for debugging)
  getStaleAnimals: () => {
    const now = Date.now();
    const stale = [];
    for (const animal in updateTimestamps) {
      if (now - updateTimestamps[animal] > ANIMAL_TIMEOUT) {
        stale.push({
          animal,
          lastUpdated: new Date(updateTimestamps[animal]).toISOString(),
          ageSeconds: Math.floor((now - updateTimestamps[animal]) / 1000)
        });
      }
    }
    return stale;
  }
};

export { dataStore, Storage, ANIMAL_TIMEOUT };
