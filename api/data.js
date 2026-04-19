import { Storage } from './storage.js';

// Global store to hold animals during runtime
global.animalStore = global.animalStore || {};
global.animalTimestamps = global.animalTimestamps || {};

const ANIMAL_TIMEOUT = 10000; // 10 seconds

export default async function handler(req, res) {
  // Enable CORS
  res.setHeader('Access-Control-Allow-Origin', '*');
  res.setHeader('Access-Control-Allow-Methods', 'GET, POST, OPTIONS');
  res.setHeader('Access-Control-Allow-Headers', 'Content-Type');
  res.setHeader('Content-Type', 'application/json');
  
  // Prevent caching
  res.setHeader('Cache-Control', 'no-cache, no-store, must-revalidate, max-age=0');
  res.setHeader('Pragma', 'no-cache');
  res.setHeader('Expires', '0');

  if (req.method === 'OPTIONS') {
    res.status(200).end();
    return;
  }

  if (req.method === "GET") {
    try {
      // Get active animals from Redis (or memory fallback)
      const result = await Storage.getAll();
      const count = Object.keys(result).length;
      
      console.log(`📤 Sending ${count} active animals`);
      return res.status(200).json(result);

    } catch (error) {
      console.error('❌ Data Retrieval Error:', error.message);
      return res.status(500).json({ 
        error: "Internal server error",
        message: error.message
      });
    }
  } else {
    res.status(405).json({ error: "Method not allowed. Use GET." });
  }
}