import { dataStore } from './storage.js';

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
      const animalCount = Object.keys(dataStore).length;
      
      console.log(`✅ Sending data: ${animalCount} animal(s)`);
      console.log(`📦 Current Store:`, dataStore);

      res.status(200).json({
        ...dataStore,
        _metadata: {
          count: animalCount,
          timestamp: new Date().toISOString(),
          zones: Array.from(new Set(Object.values(dataStore)))
        }
      });

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