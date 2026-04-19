import { Storage } from './storage.js';

// Persistent global store during runtime
const ANIMAL_TIMEOUT = 10000; // 10 seconds

export default function handler(req, res) {
  // Enable CORS
  res.setHeader('Access-Control-Allow-Origin', '*');
  res.setHeader('Access-Control-Allow-Methods', 'GET, POST, OPTIONS');
  res.setHeader('Access-Control-Allow-Headers', 'Content-Type');
  res.setHeader('Content-Type', 'application/json');

  // Handle preflight requests
  if (req.method === 'OPTIONS') {
    res.status(200).end();
    return;
  }

  console.log(`[${new Date().toISOString()}] 📤 ${req.method} request received`);

  if (req.method === "GET") {
    try {
      console.log('🔄 GET /api/data - Fetching animals...');
      
      // Use global store to get animals
      global.animalStore = global.animalStore || {};
      global.animalTimestamps = global.animalTimestamps || {};
      
      const now = Date.now();
      const result = {};
      
      // Clean up stale animals
      for (const animal in global.animalStore) {
        const age = now - (global.animalTimestamps[animal] || 0);
        
        if (age > ANIMAL_TIMEOUT) {
          console.log(`🗑️ Removing stale animal: ${animal} (age: ${Math.floor(age/1000)}s)`);
          delete global.animalStore[animal];
          delete global.animalTimestamps[animal];
        } else {
          result[animal] = global.animalStore[animal];
        }
      }
      
      const animalCount = Object.keys(result).length;
      console.log(`✅ Returning ${animalCount} active animals:`, result);

      res.status(200).json(result);

    } catch (error) {
      console.error('❌ Error sending data:', error.message);
      res.status(500).json({ 
        error: "Internal server error",
        message: error.message,
        timestamp: new Date().toISOString()
      });
    }
  } else {
    console.warn(`⚠️ Method not allowed: ${req.method}`);
    res.status(405).json({ 
      error: "Method not allowed",
      allowed: ["GET"],
      timestamp: new Date().toISOString()
    });
  }
}