const CACHE = 'vocalpro-v7';
const ASSETS = [
  '/Vocalpro-app/manifest.json',
  '/Vocalpro-app/icon-512.png',
  'https://fonts.googleapis.com/css2?family=Outfit:wght@300;400;600;700;800;900&family=Playfair+Display:wght@700;800&display=swap',
];

self.addEventListener('install', e => {
  e.waitUntil(
    caches.open(CACHE).then(c => c.addAll(ASSETS)).then(() => self.skipWaiting())
  );
});

self.addEventListener('activate', e => {
  e.waitUntil(
    caches.keys().then(keys =>
      Promise.all(keys.filter(k => k !== CACHE).map(k => caches.delete(k)))
    ).then(() => self.clients.claim())
  );
});

self.addEventListener('fetch', e => {
  const url = e.request.url;
  // Ne jamais cacher index.html — toujours réseau
  if (url.includes('/Vocalpro-app/') && (url.endsWith('/') || url.includes('index.html'))) return;
  // Ne pas intercepter Firebase / Google API
  if (url.includes('firebaseapp') || url.includes('googleapis.com/identitytoolkit')
    || url.includes('firestore') || url.includes('translate.google')) return;
  e.respondWith(
    caches.match(e.request).then(cached => {
      if (cached) return cached;
      return fetch(e.request).then(res => {
        if (!res || res.status !== 200 || res.type === 'opaque') return res;
        const clone = res.clone();
        caches.open(CACHE).then(c => c.put(e.request, clone));
        return res;
      });
    })
  );
});
