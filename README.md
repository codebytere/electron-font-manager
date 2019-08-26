# electron-font-manager

## Description

```js
$ npm i electron-font-manager
```

This native Node.js module allows you to manipulate fonts on macOS.

*Here there be (some) dragons - this is a work in progress. Please try out with caution.*

## API

### fontManager.getAvailableFonts([params])

* `params` Object (optional)
  * `traits` Array<String> - An array of strings specifying what traits to filter the available system fonts for; must be one of: 'bold', 'compressed', 'condensed', 'expanded', 'fixedPitch', 'italic', 'narrow', 'nonStandardCharacterSet', 'poster', 'smallCaps', 'unbold', 'unitalic'.

Returns the names of the fonts available in the system.

These pairs of traits are mutually exclusive:
* 'condensed' and 'expanded'
* 'bold' and 'unbold'
* 'italic' and 'unitalic'

```js
const { getAvailableFonts } = require('electron-font-manager')

const availableFonts = getAvailableFonts({
  traits: ['bold', 'italic', 'condensed']
})

console.log(availableFonts)
/* prints:
[
  'AvenirNextCondensed-DemiBoldItalic',
  'AvenirNextCondensed-BoldItalic',
  'AvenirNextCondensed-HeavyItalic'
]
*/
```

### fontManager.getAvailableFontFamilies()

Returns Array<String> - the names of the font families available in the system.

```js
const { getAvailableFontFamilies } = require('electron-font-manager')

const availableFontFamilies = getAvailableFontFamilies()

console.log(availableFontFamilies)
/* prints:
[
  'Al Bayan',
  'Al Nile',
  'Al Tarikh',
  'American Typewriter',
  'Andale Mono',
  'Arial',
  ... 241 more items
]
*/
```

### fontManager.getAvailableMembersOfFontFamily(fontFamily)

* `family` String - The name of a font family, like one returned in `availableFontFamilies()`.

Returns Array<Array> - an array with one array entry for each available member of a font family.

Each entry of the returned array is another array with three members, as follows:

1. The font name, as a string.
2. The part of the font name used in the font panel that’s not the font name, as a string. This value is not localized—for example, 'Roman', 'Italic', or 'Bold'.
2. The font’s weight, as a number.

```js
const fontManager = require('electron-font-manager')

const members = fontManager.getAvailableMembersOfFontFamily('Times')

console.log(members)
/* prints:
[
 ['Times-Roman', 'Roman', 5],
 ['Times-Italic', 'Italic', 6],
 ['Times-Bold', 'Bold', 9],
 ['Times-BoldItalic', 'Bold Italic', 9]
]
*/
```

### fontManager.showFontPanel(showStyles)

* `showStyles` Boolean - whether to open the font styles panel or the regular font panel.

```js
const fontManager = require('electron-font-manager')

// shows the font styles panel
fontManager.showFontPanel(true)
```
