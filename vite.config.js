import { defineConfig } from 'vite'
import react from '@vitejs/plugin-react'

// https://vitejs.dev/config/
export default defineConfig({
  plugins: [react()],
  server: {
    host: true,
    allowedHosts: [
      '5173-im7qo4pyevvoc5t7ldxka-b8c70a5c.manus.computer'
    ]
  }
});


