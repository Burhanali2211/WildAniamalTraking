import { Storage } from './storage.js';

// Global store to hold animals during runtime
global.animalStore = global.animalStore || {};
global.animalTimestamps = global.animalTimestamps || {};

const ANIMAL_TIMEOUT = 10000; // 10 seconds

export default function handler(req, res) {
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

  if (req.method === "POST") {
    try {
      let { animal, zone } = req.body;

      // Log raw request for debugging
      console.log(`[${new Date().toISOString()}] 📥 POST Data:`, req.body);

      // Validate input
      if (!animal || !zone) {
        console.error('❌ Missing fields. Body was:', req.body);
        return res.status(400).json({ 
          error: "Missing fields: animal and zone",
          received: req.body,
          timestamp: new Date().toISOString()
        });
      }

      // Cleanup and normalize
      animal = String(animal).trim();
      zone = String(zone).trim();

      // Store in global object
      global.animalStore[animal] = zone;
      global.animalTimestamps[animal] = Date.now();

      // Also update via Storage module for redundancy
      Storage.set(animal, zone);

      console.log(`✅ Update Successful: ${animal} @ ${zone}`);

      return res.status(200).json({ 
        status: "ok",
        message: `Updated ${animal}`,
        timestamp: new Date().toISOString()
      });

    } catch (error) {
      console.error('❌ Update Error:', error.message);
      return res.status(500).json({ 
        error: "Internal server error",
        message: error.message
      });
    }
  } else {
    res.status(405).json({ error: "Method not allowed. Use POST." });
  }
}