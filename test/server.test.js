const nsr = require('..');
const net = require('net');

describe('server', () => {
  test('accepts options', done => {
    const s = new nsr.Server((m) => {
      expect(m).toBe('world!\r\n');
      s.close();
      c.destroy();
      done();
    });
    s.listen();
    const c = net.connect({port: 8080}, () => {
      c.write('world!\r\n');
      // expect(cb).toHaveBeenCalled();
    });
  });
});