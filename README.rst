ketama
======

Python bindings for `Ketama consistent hash library`_.

.. _Ketama consistent hash library: https://github.com/RJ/ketama

Example
-------

.. code-block:: python

    from ketama import Ketama

    with open("ketama.conf", "w") as fp:
        fp.write("127.0.0.1:1\t400\n")
        fp.write("127.0.0.1:2\t600\n")
        fp.write("127.0.0.1:3\t600\n")


    def main():
        k = Ketama("ketama.conf")


        key_hash, server = k.get_server("1")
        assert server == "127.0.0.1:3"

        key_hash, server = k.get_server("3")
        assert server != "127.0.0.1:1"


    if __name__ == '__main__':
        main()
