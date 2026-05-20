const cards = Array.from(document.querySelectorAll('.card'));
const angleSelect = document.getElementById('angleFilter');
const tauSelect = document.getElementById('tauFilter');

cards.sort((a, b) => Number(a.dataset.tau) - Number(b.dataset.tau));
const grid = document.getElementById('grid');
cards.forEach(card => grid.appendChild(card));

const angles = [...new Set(cards.map(c => c.dataset.angle))].sort((a,b)=>a-b);
const taus   = [...new Set(cards.map(c => c.dataset.tau))].sort((a,b)=>a-b);

for (const a of angles) angleSelect.innerHTML += `<option value="${a}">${a}°</option>`;
for (const t of taus)   tauSelect.innerHTML += `<option value="${t}">${t}</option>`;

function applyFilter() {
  const a = angleSelect.value;
  const t = tauSelect.value;
  for (const c of cards) {
    const okA = (a === 'all' || c.dataset.angle === a);
    const okT = (t === 'all' || c.dataset.tau === t);
    c.classList.toggle('hidden', !(okA && okT));
  }
}

angleSelect.addEventListener('change', applyFilter);
tauSelect.addEventListener('change', applyFilter);
applyFilter();

// Lightbox
const lightbox = document.getElementById('lightbox');
const lightboxImg = document.getElementById('lightbox-img');

cards.forEach(card => {
  const img = card.querySelector('img');
  img.addEventListener('click', () => {
    lightboxImg.src = img.src;
    lightbox.style.display = "flex";
  });
});

lightbox.addEventListener('click', () => {
  lightbox.style.display = "none";
});
