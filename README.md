# Swadge-Parts

A KiCad library of parts used by Magfest's Swadges.

Prices may be automatically updated from LCSC using this Python script:
```bash
# Install requirements
python -m pip install -r requirements.txt

# Update prices. This will rewrite Swadge_Parts.kicad_sym
python getLcscPrices.py
```