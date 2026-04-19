// Shared data storage for all API routes
// In production, use Redis or a database instead

let dataStore = {};
let updateTimestamps = {};

const Storage = {
  // Set animal data
  set: (animal, zone) => {
    dataStore[animal] = zone;
    updateTimestamps[animal] = new Date().toISOString();
  },

  // Get all animal data
  getAll: () => {
    return { ...dataStore };
  },

  // Get single animal
  get: (animal) => {
    return dataStore[animal] || null;
  },

  // Get with timestamps
  getAllWithTimestamps: () => {
    const result = {};
    for (const animal in dataStore) {
      result[animal] = {
        zone: dataStore[animal],
        lastUpdated: updateTimestamps[animal]
      };
    }
    return result;
  },

  // Clear all data
  clear: () => {
    dataStore = {};
    updateTimestamps = {};
  },

  // Delete specific animal
  delete: (animal) => {
    delete dataStore[animal];
    delete updateTimestamps[animal];
  },

  // Check if animal exists
  has: (animal) => {
    return animal in dataStore;
  },

  // Get count
  count: () => {
    return Object.keys(dataStore).length;
  }
};

export { dataStore, Storage };
