# 🐾 SKICC Wildlife Tracking System (v2.0)

A professional, high-precision animal tracking system using a Mesh-Gateway architecture (ESP-NOW) and a Cloud-HUD dashboard.

## 📁 Repository Structure
- **`/api`**: Node.js Serverless Backend (Vercel) with Redis Persistence.
- **`/dashboard`**: Premium Dark HUD Dashboard (HTML/JS/Leaflet).
- **`/firmware`**: Arduino Code for ESP32/ESP8266.
  - `/animal_unit`: Code for the belt trackers (Broadcast mode).
  - `/gateway_node`: Code for the 5 server nodes (Main Hall, North Lawn, etc.).

## 🚀 Deployment Guide

### 1. Cloud Setup (Vercel)
- Deploy the project to Vercel.
- Connect your **Upstash Redis** (Add `UPSTASH_REDIS_REST_URL` and `UPSTASH_REDIS_REST_TOKEN` in Vercel settings).

### 2. Hardware Setup
- **Animals**: Flash `animal_unit.ino` to each ESP8266 belt.
- **Gateways**: Flash `gateway_node.ino` to your 5 ESP32 servers. 
  - **Important**: Change `GATEWAY_ZONE` name in the code for each of the 5 servers to match their location!

### 3. Monitoring
- Open your Vercel URL to see the live map and observation logs.

---
**Status**: ✅ Operational (Production Ready)
