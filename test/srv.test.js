const nsr = require('../');

test('has Server constructor function', () => {
    expect(typeof nsr.Server).toBe('function');
});

test('instantiates Server', () => {
    const s = new nsr.Server();
    expect(s instanceof nsr.Server).toBe(true);
})