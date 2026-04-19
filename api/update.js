import { Storage } from './storage.js';

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

  console.log(`[${new Date().toISOString()}] 📥 ${req.method} request received`);

  if (req.method === "POST") {
    try {
      const { animal, zone } = req.body;

      // Validate input
      if (!animal || !zone) {
        console.error('❌ Missing required fields:', { animal, zone });
        res.status(400).json({ 
          error: "Missing required fields: animal and zone",
          timestamp: new Date().toISOString()
        });
        return;
      }

      // Validate types
      if (typeof animal !== 'string' || typeof zone !== 'string') {
        console.error('❌ Invalid data types');
        res.status(400).json({ 
          error: "Invalid data types - both must be strings",
          timestamp: new Date().toISOString()
        });
        return;
      }

      // Update storage with timestamp
      Storage.set(animal, zone);

      console.log(`✅ Updated: ${animal} → ${zone}`);
      console.log(`📦 Active animals: ${Storage.count()}`);

      res.status(200).json({ 
        status: "ok",
        message: `Successfully updated ${animal}`,
        timestamp: new Date().toISOString(),
        data: { animal, zone }
      });

    } catch (error) {
      console.error('❌ Error processing request:', error.message);
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
      allowed: ["POST"],
      timestamp: new Date().toISOString()
    });
  }
}