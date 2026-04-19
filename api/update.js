let dataStore = {};

export default function handler(req, res) {
  console.log("📥 Incoming request:", req.method);

  if (req.method === "POST") {
    const { animal, zone } = req.body;

    console.log("📡 Data received:", animal, zone);

    dataStore[animal] = zone;

    console.log("📦 Current Store:", dataStore);

    res.status(200).json({ status: "ok" });
  } else {
    res.status(405).json({ error: "Method not allowed" });
  }
}