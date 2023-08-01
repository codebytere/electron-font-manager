const { expect } = require('chai')
const {
  getAvailableFontFamilies,
  getAvailableMembersOfFontFamily,
  getAvailableFonts,
  FontPanel
} = require('../index')

describe('electron-font-manager', () => {
  describe('getAvailableFonts()', () => {
    it('should throw if params is not an object', () => {
      expect(() => {
        getAvailableFonts(12345)
      }).to.throw(/params must be an object/)
    })

    it('should throw if params.traits is not an array', () => {
      expect(() => {
        getAvailableFonts({traits: 12345})
      }).to.throw(/traits must be an array/)
    })

    it('should throw if params.traits contains invalid traits', () => {
      expect(() => {
        getAvailableFonts({traits: ['bad-trait']})
      }).to.throw(/Invalid trait type/)
    })

    it('should return an array', () => {
      const fonts = getAvailableFonts()
      expect(fonts).to.be.an('array')
    })
  })

  describe('getAvailableFontFamilies()', () => {
    it('should not throw', () => {
      expect(() => {
        getAvailableFontFamilies()
      }).to.not.throw()
    })

    it('should return an array', () => {
      const fonts = getAvailableFontFamilies()
      expect(fonts).to.be.an('array')
    })
  })

  describe('getAvailableMembersOfFontFamily(fontFamily)', () => {
    it('should throw if fontFamily is not a string', () => {
      expect(() => {
        getAvailableMembersOfFontFamily(12345)
      }).to.throw(/fontFamily must be a string/)
    })

    it('should return an array', () => {
      const members = getAvailableMembersOfFontFamily('Helvetica')
      expect(members).to.be.an('array')
    })
  })

  describe('FontPanel.show({ showStyles })', () => {
    it('should throw if showStyles is not a boolean', () => {
      expect(() => {
        const panel = new FontPanel();
        panel.show({ showStyles: 'hello' });
      }).to.throw(/showStyles must be a boolean/)
    })
  })
})
