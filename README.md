# electron-font-manager

## Description

```js
$ npm i electron-font-manager
```

This native Node.js module allows you to manipulate fonts on macoS.

## API

### fontManager.getAvailableFonts([params]) _macOS_

* `params` Object (optional)
  * `traits` Array<String> - An array of strings specifying what traits to filter the available system fonts for; must be one of: 'bold', 'compressed', 'condensed', 'expanded', 'fixedPitch', 'italic', 'narrow', 'nonStandardCharacterSet', 'poster', 'smallCaps', 'unbold', 'unitalic'.

Returns the names of the fonts available in the system.

These pairs of traits are mutually exclusive:
* 'condensed' and 'expanded'
* 'bold' and 'unbold'
* 'italic' and 'NSUnitalicFontMask'

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

### fontManager.getAvailableFontFamilies() _macOS_

Returns the names of the font families available in the system.

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
