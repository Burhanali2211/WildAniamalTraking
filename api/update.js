let dataStore = {};

export default function handler(req, res) {
  if (req.method === "POST") {
    const { animal, zone } = req.body;

    dataStore[animal] = zone;

    res.status(200).json({ status: "ok" });
  } else {
    res.status(405).end();
  }
}