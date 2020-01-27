import tempfile

import pytest

from ketama import Ketama


@pytest.fixture
def ketama_config():
    with tempfile.NamedTemporaryFile(suffix="_ketama.conf", mode="w") as fp:
        fp.write("127.0.0.1:1\t400\n")
        fp.write("127.0.0.1:2\t600\n")
        fp.write("127.0.0.1:3\t600\n")
        fp.flush()

        yield fp.name


def test_init(ketama_config):
    with pytest.raises(ValueError):
        Ketama()

    with pytest.raises(ValueError):
        Ketama("")

    with pytest.raises(RuntimeError):
        Ketama("foo")

    Ketama(ketama_config)


def test_consistent_hash(ketama_config):
    k = Ketama(ketama_config)

    key_hash, server = k.get_server(b"")
    assert server == "127.0.0.1:3"

    key_hash, server = k.get_server("")
    assert server == "127.0.0.1:3"

    key_hash, server = k.get_server("1")
    assert server == "127.0.0.1:3"

    key_hash, server = k.get_server(b"1")
    assert server == "127.0.0.1:3"

    key_hash, server = k.get_server(b"1488")
    assert server != "127.0.0.1:1"

    key_hash, server = k.get_server(b"\0\0\0\0\0\0\0\0\0")
    assert server == "127.0.0.1:1"

    key_hash, server = k.get_server(b"\0\0\0\0\0\0\0")
    assert server == "127.0.0.1:2"
