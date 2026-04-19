# 🚀 Deployment & Troubleshooting Guide

## System Architecture

```
Animals (ESP8266)           Server (ESP8266)              Dashboard (Web)
     ↓                           ↓                              ↓
ESP-NOW Broadcast      WiFi → HTTP POST             Fetch /api/data
  (broadcasts name)     /api/update (stores data)   Every 3 seconds
                        ↓
                    storage.js (shared)
                        ↓
                    data.js (retrieves)
```

## 📋 Pre-Deployment Checklist

### Arduino Code (animals.ino)
- [ ] WiFi disabled (using only ESP-NOW)
- [ ] Broadcast address set to `FF:FF:FF:FF:FF:FF`
- [ ] Device name in `DEVICE_NAME` matches your animal
- [ ] Loop interval is 5000ms (5 seconds)

### Arduino Code (serverofanimal.ino)
- [ ] WiFi SSID and password correct
- [ ] API endpoint: `https://your-domain.vercel.app/api/update`
- [ ] RSSI zones configured correctly
- [ ] BUZZER pin (5) and SWITCH pin (0) correct for your board

### Backend Configuration
- [ ] Vercel.json routes configured
- [ ] API endpoint URLs are public and accessible
- [ ] CORS headers enabled

### Dashboard
- [ ] API_ENDPOINT points to correct `/api/data`
- [ ] FETCH_INTERVAL set to 3000ms
- [ ] Zones match your SKICC layout

---

## 🔧 Troubleshooting

### Issue: Animals not sending data
**Symptoms**: Dashboard shows "No animals detected yet"

**Solutions**:
1. Check ESP-NOW broadcast is working
   ```
   Serial monitor should show: "✅ Send Success"
   ```
2. Verify broadcast address: `FF:FF:FF:FF:FF:FF`
3. Check device sleeps for less than 5 seconds
4. Ensure setup() initializes properly

### Issue: Server not receiving data
**Symptoms**: Animals broadcast, but API doesn't get data

**Solutions**:
1. Check WiFi connection on server ESP
   ```
   Serial: "✅ WiFi Connected"
   ```
2. Verify API endpoint URL in serverofanimal.ino
3. Check request format:
   ```json
   {"animal":"Billi_Meow","zone":"SKICC_MainHall"}
   ```
4. Monitor server logs in Vercel dashboard

### Issue: Dashboard not updating
**Symptoms**: Dashboard shows "Offline - Retrying..."

**Solutions**:
1. Check API is accessible:
   ```bash
   curl https://your-domain.vercel.app/api/data
   ```
2. Verify response format is JSON
3. Check browser console for CORS errors
4. Verify `/api/update` and `/api/data` routes exist

### Issue: RSSI mapping to zones is wrong
**Problem**: Animals show in wrong zones

**Solution**: Adjust RSSI thresholds in `getZoneFromRSSI()`:
```cpp
String getZoneFromRSSI(int rssi) {
  if (rssi > -50) return "SKICC_MainHall";  // Strongest signal
  if (rssi > -60) return "SKICC_NorthLawn";
  if (rssi > -70) return "SKICC_SouthLawn";
  if (rssi > -80) return "SKICC_Parking";   // Weakest signal
  return "SKICC_Lakeside";
}
```

---

## 📊 Monitoring

### Check Server Status
Visit Vercel dashboard → Function logs → Select `/api/data` or `/api/update`

### View Real-time Dashboard Logs
- Dashboard shows connection log in sidebar
- Browser DevTools Console → Check for errors
- Serial monitor on Arduino boards

### Monitor Data Flow
1. Arduino sends: `"animal":"name","zone":"zone"`
2. Server receives: `/api/update` POST request
3. Server stores in memory
4. Dashboard fetches: `/api/data` GET request
5. Dashboard displays: Real-time markers & table

---

## 🔒 Security Notes

⚠️ **Current Implementation**: Data stored in server memory (volatile)

**For Production**:
1. Use Supabase/Firebase for persistent storage
2. Add authentication tokens
3. Implement rate limiting
4. Add SSL certificates (already in Vercel)
5. Sanitize all inputs before storage

---

## 📈 Performance Optimization

- Dashboard fetches every 3 seconds (configurable)
- Max 3 retry attempts on failed requests
- 5-second timeout per request
- Connection logs limited to 20 entries
- Marker clustering disabled (5 zones)

---

## 🚀 Deployment Steps

1. Update Arduino code with your credentials
2. Upload to both ESP8266 boards
3. Update Vercel API endpoint in serverofanimal.ino
4. Deploy updated files to Vercel:
   ```bash
   git add .
   git commit -m "Update dashboard and API"
   git push
   ```
5. Monitor Vercel logs for errors
6. Check dashboard for real-time updates

---

## 📞 Quick Support

| Issue | Check | Fix |
|-------|-------|-----|
| No data | Serial monitor | Verify SSID/WiFi |
| Wrong zone | RSSI values | Adjust thresholds |
| API errors | Vercel logs | Check endpoint URL |
| Dashboard blank | API response | Verify data format |

---

**Last Updated**: April 19, 2026
**Status**: ✅ All systems operational
