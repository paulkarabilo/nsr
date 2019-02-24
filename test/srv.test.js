const nsr = require('../');

test('has Server constructor function', () => {
    expect(typeof nsr.Server).toBe('function');
});

test('instantiates Server', () => {
    const s = new nsr.Server();
    expect(s instanceof nsr.Server).toBe(true);
});

test('emits events', () => {
    const s = new nsr.Server();
    const fn = jest.fn();

    s.on('event', fn);
    s.emit('event', 'test');
    expect(fn).toHaveBeenCalledTimes(1);
    expect(fn).toHaveBeenCalledWith('test');
});

test('sets multiple listeners', () => {
    const s = new nsr.Server();
    const fn = jest.fn()
    Array(200).fill(0).forEach((v, i) => {
        s.on('event', fn);
    });
    s.emit('event', 'test');
    expect(fn).toHaveBeenCalledTimes(200);
    expect(fn).toHaveBeenCalledWith('test');
})