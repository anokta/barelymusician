const fs = require('fs').promises;
const path = require('path');
const {execSync} = require('child_process');

(async () => {
  const rootDir = __dirname;
  const demoDir = path.join(__dirname, 'demo');
  const distDir = path.join(__dirname, 'dist');

  await fs.mkdir(distDir, {recursive: true});

  // Copy index.html
  const indexPath = path.join(demoDir, 'index.html');
  try {
    let index = await fs.readFile(indexPath, 'utf8');
    index = index.replace(/main\.js/g, 'bundle.js');
    await fs.writeFile(path.join(distDir, 'index.html'), index, 'utf8');
  } catch (err) {
    console.error('failed to copy index.html:', err.message);
  }

  // Copy style.css
  const cssSrc = path.join(demoDir, 'style.css');
  try {
    await fs.copyFile(cssSrc, path.join(distDir, 'style.css'));
  } catch (err) {
    console.error('failed to copy style.css:', err.message);
  }

  // Replace processor.js path
  const bundlePath = path.join(distDir, 'bundle.js');
  try {
    let bundle = await fs.readFile(bundlePath, 'utf8');
    const originalPath = '../src/processor.js';
    if (bundle.includes(originalPath)) {
      bundle = bundle.split(originalPath).join('./processor.js');
      await fs.writeFile(bundlePath, bundle, 'utf8');
    } else {
      console.error('failed to find processor.js path');
    }
  } catch (err) {
    console.error('failed to replace processor.js path', err.message);
  }

  // Add build info into index.html
  async function addBuildInfo(indexDir) {
    try {
      let index = await fs.readFile(indexDir, 'utf8');

      // Gather build info.
      let version = 'v0.0.0';
      try {
        const pkg = JSON.parse(await fs.readFile(path.join(rootDir, 'package.json'), 'utf8'));
        if (pkg && pkg.version) {
          version = pkg.version;
        } else {
          console.error('failed to parse package.json');
        }
      } catch (err) {
        console.error('failed to fetch version: ', err);
      }

      let sha = '';
      try {
        sha = execSync('git rev-parse --short HEAD', {cwd: rootDir}).toString().trim();
      } catch (err) {
        console.error('failed to fetch sha: ', err);
      }

      const escapeHtml = (s) => s.replace(/&/g, '&amp;')
                                    .replace(/</g, '&lt;')
                                    .replace(/>/g, '&gt;')
                                    .replace(/"/g, '&quot;');

      const timestamp = new Date().toISOString();
      const buildInfo = escapeHtml(`${version} • ${sha} • ${timestamp}`);

      // Add meta tag.
      index = index.replace(/<\/head>/i, `<meta name="build" content="${buildInfo}">\n</head>`);

      // Add build info.
      index = index.replace(
          /(<div[^>]*class=["']about["'][^>]*>)([\s\S]*?)(<\/div>)/i,
          `$1$2\n <small class="build">${buildInfo}</small>\n  $3`);

      await fs.writeFile(indexDir, index, 'utf8');
    } catch (err) {
      console.error('failed to add build info:', err.message);
    }
  }
  await addBuildInfo(path.join(distDir, 'index.html'));
})();
