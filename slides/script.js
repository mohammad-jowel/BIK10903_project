const slides = [...document.querySelectorAll('.slide')];
const dots = [...document.querySelectorAll('.side-progress .dot')];
const counter = document.getElementById('slideCounter');
const nextBtn = document.getElementById('nextBtn');
const prevBtn = document.getElementById('prevBtn');
let currentSlide = 0;

function pad(number) {
  return String(number).padStart(2, '0');
}

function showSlide(index) {
  currentSlide = (index + slides.length) % slides.length;

  slides.forEach((slide, i) => {
    slide.classList.toggle('active', i === currentSlide);
  });

  dots.forEach((dot, i) => {
    dot.classList.toggle('active', i === currentSlide);
  });

  counter.textContent = `${pad(currentSlide + 1)} / ${pad(slides.length)}`;
}

nextBtn.addEventListener('click', () => showSlide(currentSlide + 1));
prevBtn.addEventListener('click', () => showSlide(currentSlide - 1));

// Keyboard navigation: use ArrowRight / Space for next, ArrowLeft for previous.
document.addEventListener('keydown', (event) => {
  if (event.key === 'ArrowRight' || event.key === ' ') showSlide(currentSlide + 1);
  if (event.key === 'ArrowLeft') showSlide(currentSlide - 1);
});

// Click side dots to jump between slides.
dots.forEach((dot, index) => {
  dot.addEventListener('click', () => showSlide(index));
});

showSlide(0);
