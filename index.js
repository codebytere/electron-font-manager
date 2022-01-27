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
      throw new TypeError('Invalid trait type')
    }
  }

  return fontManager.getAvailableFonts.call(this, params)
}

function getAvailableMembersOfFontFamily(fontFamily) {
  if (typeof fontFamily !== 'string') {
    throw new TypeError('fontFamily must be a string')
  }

  return fontManager.getAvailableMembersOfFontFamily.call(this, fontFamily)
}

function showFontPanel(showStyles = false, title = null) {
  if (typeof showStyles !== 'boolean') {
    throw new TypeError('showStyles must be a boolean')
  }

  return fontManager.showFontPanel.call(this, showStyles, title || "")
}

module.exports = {
  getAvailableFonts,
  getAvailableMembersOfFontFamily,
  getAvailableFontFamilies: fontManager.getAvailableFontFamilies,
  showFontPanel,
}
