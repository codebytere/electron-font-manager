const fontManager = require('./build/Release/font_manager.node')

function getAvailableFonts(params = {}) {
  const options = [
    'bold',
    'compressed',
    'condensed',
    'expanded',
    'fixedPitch',
    'italic',
    'narrow',
    'nonStandardCharacterSet',
    'poster',
    'smallCaps',
    'unbold',
    'unitalic'
  ]

  if (typeof params !== 'object') {
    throw new TypeError('params must be an object')
  } else if (params.traits) {
    if (!Array.isArray(params.traits)) {
      throw new TypeError('traits must be an array')
    } else if (!params.traits.every(t => options.includes(t))) {
      throw new TypeError(`Invalid trait type: ${t}`)
    }
  }

  return fontManager.getAvailableFonts.call(this, params)
}

module.exports = {
  getAvailableFonts,
  getAvailableFontFamilies: fontManager.getAvailableFontFamilies
}