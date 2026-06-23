#include <cassert>
#include "ringbuffer.cpp"

int main() {
    // basic write and read
    RingBuffer<int> rb(4);
    assert(rb.is_empty());
    assert(!rb.is_full());

    rb.write(1);
    assert(!rb.is_empty());
    assert(rb.read() == 1);
    assert(rb.is_empty());

    // fifo order
    rb.write(10);
    rb.write(20);
    rb.write(30);
    assert(rb.read() == 10);
    assert(rb.read() == 20);
    assert(rb.read() == 30);

    // full buffer
    rb.write(1);
    rb.write(2);
    rb.write(3);
    rb.write(4);
    assert(rb.is_full());
    assert(!rb.write(5));

    // read frees a slot
    rb.read();
    assert(rb.write(5));

    // empty reads return nullopt
    RingBuffer<int> rb2(4);
    assert(!rb2.read().has_value());
    assert(!rb2.peek().has_value());

    // peek does not consume
    rb2.write(42);
    assert(rb2.peek() == 42);
    assert(!rb2.is_empty());
    assert(rb2.read() == 42);
    assert(rb2.is_empty());

    // wrap around
    RingBuffer<int> rb3(4);
    for (int i = 0; i < 4; i++) rb3.write(i);
    for (int i = 0; i < 4; i++) rb3.read();
    for (int i = 0; i < 4; i++) rb3.write(i * 10);
    for (int i = 0; i < 4; i++) assert(rb3.read() == i * 10);
}
