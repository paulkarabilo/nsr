const nsr = require('../');

test('has Server constructor function', () => {
    expect(typeof nsr.Server).toBe('function');
});

test('instantiates Server', () => {
    const s = new nsr.Server();
    expect(s instanceof nsr.Server).toBe(true);
});

test('emits events', done => {
    const s = new nsr.Server();
    s.on('event', (val) => {
        expect(val).toBe('test');
        done();
    });
    s.emit('event', 'test');
});

test('sets multiple listeners', done => {
    const s = new nsr.Server();
    Array(129).fill(0).forEach((v, i) => {
        s.on('event', (val) => {
            expect(val).toBe('test');
            if (i === 128) done();
        });
    });
    s.emit('event', 'test');
})