const nsr = require('../');

test('has Server constructor function', () => {
    expect(typeof nsr.Server).toBe('function');
});

test('instantiates Server', () => {
    const s = new nsr.Server();
    expect(s instanceof nsr.Server).toBe(true);
});

test('throws error on wron arguments', () => {
    const s = new nsr.Server();
    expect(() => s.on('a')).toThrow();
    expect(() => s.on()).toThrow();
    expect(() => s.on('a', 'a')).toThrow();
    expect(() => s.on(1, 1)).toThrow();
    expect(() => s.on('a', 1)).toThrow();
    expect(() => s.on('a', () => {}, 'a')).toThrow();
    expect(() => s.on('a', () => {})).not.toThrow()
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
});

test('servers events are isolated', () => {
    const s1 = new nsr.Server();
    const fn1 = jest.fn();
    const s2 = new nsr.Server();
    const fn2 = jest.fn();

    s1.on('event1', fn1);
    s2.on('event2', fn2);

    s1.emit('event1', 'test1');
    s2.emit('event1', 'test1');
    expect(fn1).toHaveBeenCalledWith('test1');
    expect(fn1).toHaveBeenCalledTimes(1);
    expect(fn2).not.toHaveBeenCalled();

    s1.emit('event2', 'test2');
    s2.emit('event2', 'test2');
    expect(fn1).toHaveBeenCalledWith('test1');
    expect(fn1).toHaveBeenCalledTimes(1);
    expect(fn2).toHaveBeenCalledWith('test2');
    expect(fn2).toHaveBeenCalledTimes(1);
})