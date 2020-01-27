from typing import Union, Tuple, List

__doc__ = """
We wrote ketama to replace how our memcached clients mapped keys to servers.
Previously, clients mapped keys->servers like this:

	server = serverlist[hash(key)%serverlist.length];

This meant that whenever we added or removed servers from the pool, everything
hashed to different servers, which effectively wiped the entire cache.

Ketama solves this problem in the following way:

 * Take your list of servers (eg: 1.2.3.4:11211, 5.6.7.8:11211, 9.8.7.6:11211)
 * Hash each server string to several (100-200) unsigned ints
 * Conceptually, these numbers are placed on a circle called the continuum.
   (imagine a clock face that goes from 0 to 2^32)
 * Each number links to the server it was hashed from, so servers appear
   at several points on the continuum, by each of the numbers they hashed to.
 * To map a key->server, hash your key to a single unsigned int, and find the
   next biggest number on the continuum. The server linked to that number is
   the correct server for that key.
 * If you hash your key to a value near 2^32 and there are no points on the
   continuum greater than your hash, return the first server in the continuum.

If you then add or remove a server from the list, only a small proportion of
keys end up mapping to different servers.

The server file looks like this:
1.2.3.4:11211	900
5.6.7.8:11211	300
9.8.7.6:11211	1500

ip:port and weighting, \t separated, \n line endings.
Just use the number of megs allocated to the server as the weight.
The weightings are realised by adding more or less points to the continuum.
"""


class Ketama:
    def __init__(self, filename: Union[str, bytes]): ...
    def get_server(self, key: Union[str, bytes]) -> Tuple[int, str]: ...
    def get_points(self) -> List[Tuple[int, str]]: ...
