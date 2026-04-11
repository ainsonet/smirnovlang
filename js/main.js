/**
 * SmirnovLang Website - Main JavaScript
 */

document.addEventListener('DOMContentLoaded', () => {
    // Mobile Navigation Toggle
    const navToggle = document.getElementById('navToggle');
    const navMenu = document.querySelector('.nav-menu');

    if (navToggle) {
        navToggle.addEventListener('click', () => {
            navMenu.classList.toggle('active');
        });
    }

    // Close mobile menu when clicking a link
    document.querySelectorAll('.nav-link').forEach(link => {
        link.addEventListener('click', () => {
            navMenu.classList.remove('active');
        });
    });

    // Tab System for Examples
    const tabButtons = document.querySelectorAll('.tab-btn');
    const tabPanes = document.querySelectorAll('.tab-pane');

    tabButtons.forEach(btn => {
        btn.addEventListener('click', () => {
            const tabId = btn.dataset.tab;

            // Remove active class from all buttons
            tabButtons.forEach(b => b.classList.remove('active'));
            // Add active class to clicked button
            btn.classList.add('active');

            // Hide all tab panes
            tabPanes.forEach(pane => pane.classList.remove('active'));
            // Show selected tab pane
            const activePane = document.getElementById(tabId);
            if (activePane) {
                activePane.classList.add('active');
            }
        });
    });

    // Smooth scroll for anchor links
    document.querySelectorAll('a[href^="#"]').forEach(anchor => {
        anchor.addEventListener('click', function(e) {
            e.preventDefault();
            const targetId = this.getAttribute('href');
            const target = document.querySelector(targetId);
            
            if (target) {
                const offsetTop = target.offsetTop - 80; // Account for fixed navbar
                window.scrollTo({
                    top: offsetTop,
                    behavior: 'smooth'
                });
            }
        });
    });

    // Navbar scroll effect
    const navbar = document.querySelector('.navbar');
    let lastScroll = 0;

    window.addEventListener('scroll', () => {
        const currentScroll = window.pageYOffset;

        if (currentScroll > 100) {
            navbar.style.background = 'rgba(22, 27, 34, 0.95)';
            navbar.style.backdropFilter = 'blur(10px)';
        } else {
            navbar.style.background = 'var(--bg-secondary)';
            navbar.style.backdropFilter = 'none';
        }

        lastScroll = currentScroll;
    });

    // Animate elements on scroll
    const observerOptions = {
        threshold: 0.1,
        rootMargin: '0px 0px -50px 0px'
    };

    const observer = new IntersectionObserver((entries) => {
        entries.forEach(entry => {
            if (entry.isIntersecting) {
                entry.target.style.opacity = '1';
                entry.target.style.transform = 'translateY(0)';
            }
        });
    }, observerOptions);

    // Observe feature cards and download cards
    document.querySelectorAll('.feature-card, .download-card').forEach(card => {
        card.style.opacity = '0';
        card.style.transform = 'translateY(20px)';
        card.style.transition = 'opacity 0.6s ease, transform 0.6s ease';
        observer.observe(card);
    });

    // Copy code button functionality
    const codeWindows = document.querySelectorAll('.code-window');
    
    codeWindows.forEach(window => {
        const copyBtn = document.createElement('button');
        copyBtn.className = 'copy-btn';
        copyBtn.innerHTML = '📋';
        copyBtn.title = 'Copy code';
        
        const codeContent = window.querySelector('.code-content');
        if (codeContent) {
            codeContent.appendChild(copyBtn);
        }

        copyBtn.addEventListener('click', () => {
            const code = codeContent.textContent;
            navigator.clipboard.writeText(code).then(() => {
                copyBtn.innerHTML = '✅';
                setTimeout(() => {
                    copyBtn.innerHTML = '📋';
                }, 2000);
            });
        });
    });

    // Add copy button styles dynamically
    const style = document.createElement('style');
    style.textContent = `
        .copy-btn {
            position: absolute;
            top: 10px;
            right: 10px;
            background: var(--bg-tertiary);
            border: 1px solid var(--border-color);
            border-radius: 6px;
            padding: 5px 10px;
            cursor: pointer;
            font-size: 1rem;
            opacity: 0.7;
            transition: all 0.3s;
        }
        
        .copy-btn:hover {
            opacity: 1;
            background: var(--accent-primary);
        }
        
        .code-window {
            position: relative;
        }
    `;
    document.head.appendChild(style);

    // Typing animation for hero section (optional enhancement)
    const heroSubtitle = document.querySelector('.hero-subtitle');
    if (heroSubtitle) {
        const originalText = heroSubtitle.textContent;
        heroSubtitle.textContent = '';
        
        let i = 0;
        const typingInterval = setInterval(() => {
            if (i < originalText.length) {
                heroSubtitle.textContent += originalText.charAt(i);
                i++;
            } else {
                clearInterval(typingInterval);
            }
        }, 30);
    }

    // Console easter egg
    console.log('%c🚀 SmirnovLang', 'color: #58a6ff; font-size: 24px; font-weight: bold;');
    console.log('%cСовременный язык программирования с инновационными возможностями', 'color: #8b949e; font-size: 12px;');
    console.log('%cИсходный код: https://github.com/ainsonet/smirnovlang', 'color: #79c0ff; font-size: 12px;');
});
