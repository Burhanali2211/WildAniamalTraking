let dataStore = {};

export default function handler(req, res) {
  console.log("📤 Sending data:", dataStore);

  res.status(200).json(dataStore);
}