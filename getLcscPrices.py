#!/usr/bin/python3

from kiutils.symbol import SymbolLib
from kiutils.symbol import Symbol
from kiutils.symbol import Property
from bs4 import BeautifulSoup
from urllib.request import urlopen
import re


def getLcscCost(lcscPartNum: str, minQty: int) -> float:

    price: float = 0

    # Make sure there's a valid LCSC part number
    if (len(lcscPartNum) == 0):
        print('     No LCSC')
        return price

    # Define the URL of the webpage to scrape
    url = 'https://www.lcsc.com/product-detail/' + lcscPartNum + '.html'

    # Fetch the HTML
    with urlopen(url) as response:
        body = response.read().decode()

        # Parse the HTML content using BeautifulSoup
        soup = BeautifulSoup(body, 'html.parser')

        # Find the price table
        priceTable = soup.find('table', {'class': 'priceTable'})

        # Make sure the price table exists
        if None is priceTable:
            print('     No price for ' + url)
            return price

        # For each row
        for row in priceTable.find_all('tr'):
            # Get the cells
            cells = row.find_all('td')
            if (len(cells) >= 3):
                # Get the quantity
                qty = int(re.sub(r'[^0-9]', '', cells[0].text))

                # Unit price is the last one, in case of sales
                unitPrices = re.sub(r'[ \t\r]+', '', cells[1].text)
                unitPrices = list(filter(None, unitPrices.split('\n')))
                unitPrice = unitPrices[-1]
                unitPrice = re.sub(r'[^0-9\.]', '', unitPrice)
                unitPrice = float(unitPrice)

                # Don't care about ext price
                # extPrice = float(re.sub(r'[.unitPrice*\$\s*]', '', cells[2].text))

                # Record the lowest price under the quantity
                if (qty <= minQty and (unitPrice < price or 0 == price)):
                    price = unitPrice

    return price


def main():
    # Load the library
    symLibFile = 'Swadge_Parts.kicad_sym'
    symbolLib: SymbolLib = SymbolLib().from_file(symLibFile)

    # For each symbol
    symIdx: int = 0
    symbol: Symbol
    for symbol in symbolLib.symbols:

        # Keep track if the price was updated
        priceUpdated: bool = False

        # For each property
        property: Property
        for property in symbol.properties:

            # Remove old Cost100 field
            if 'Cost100' == property.key:
                print('Remove Cost100 from ' + symbol.entryName)
                symbol.properties.remove(property)

            # Add new Cost@2500 field from LCSC part number
            if 'LCSC' == property.key.upper() and property.value:
                print('%3d%% Fetching price for %s (%s)' %
                      ((100 * symIdx) / float(len(symbolLib.symbols)), symbol.entryName, property.value))
                symbol.properties.append(
                    Property('Cost@2500', str(getLcscCost(property.value, 2500)), showName=False))
                priceUpdated = True

        # Print for symbols without LCSC
        if not priceUpdated:
            print('%3d%% No LCSC for %s' %
                  ((100 * symIdx) / float(len(symbolLib.symbols)), symbol.entryName))

        # Increment to track progress
        symIdx += 1

    # Write the library with the new prices
    symbolLib.to_file(symLibFile)

    print('Prices fetched from LCSC')


if __name__ == "__main__":
    main()
