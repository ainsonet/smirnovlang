/**
 * Documentation page - Active link highlighting
 */

document.addEventListener('DOMContentLoaded', () => {
    const sidebarLinks = document.querySelectorAll('.docs-nav a');
    const sections = document.querySelectorAll('article h2, article h3');

    // Highlight active section in sidebar
    window.addEventListener('scroll', () => {
        const scrollPosition = window.scrollY + 150;

        sections.forEach(section => {
            const sectionTop = section.offsetTop;
            const sectionId = section.id;

            if (scrollPosition >= sectionTop) {
                sidebarLinks.forEach(link => {
                    link.classList.remove('active');
                    if (link.getAttribute('href') === `#${sectionId}`) {
                        link.classList.add('active');
                    }
                });
            }
        });
    });

    // Smooth scroll for sidebar links
    sidebarLinks.forEach(link => {
        link.addEventListener('click', (e) => {
            e.preventDefault();
            const targetId = link.getAttribute('href');
            const target = document.querySelector(targetId);

            if (target) {
                const offsetTop = target.offsetTop - 100;
                window.scrollTo({
                    top: offsetTop,
                    behavior: 'smooth'
                });
            }
        });
    });

    // Copy code buttons
    const codeBlocks = document.querySelectorAll('pre');
    
    codeBlocks.forEach(block => {
        const copyBtn = document.createElement('button');
        copyBtn.className = 'copy-btn';
        copyBtn.innerHTML = '📋';
        copyBtn.title = 'Copy code';

        block.appendChild(copyBtn);

        copyBtn.addEventListener('click', () => {
            const code = block.querySelector('code')?.textContent || block.textContent;
            navigator.clipboard.writeText(code.trim()).then(() => {
                copyBtn.innerHTML = '✅';
                setTimeout(() => {
                    copyBtn.innerHTML = '📋';
                }, 2000);
            });
        });
    });

    // Add copy button styles
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
        
        pre {
            position: relative;
        }
    `;
    document.head.appendChild(style);
});
