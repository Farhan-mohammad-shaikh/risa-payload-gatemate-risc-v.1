ICE40
=====

Register Listing for ICE40
--------------------------

+--------------------------------------+-----------------------------------+
| Register                             | Address                           |
+======================================+===================================+
| :ref:`ICE40_TX <ICE40_TX>`           | :ref:`0xf0004000 <ICE40_TX>`      |
+--------------------------------------+-----------------------------------+
| :ref:`ICE40_RX <ICE40_RX>`           | :ref:`0xf0004004 <ICE40_RX>`      |
+--------------------------------------+-----------------------------------+
| :ref:`ICE40_BUSY <ICE40_BUSY>`       | :ref:`0xf0004008 <ICE40_BUSY>`    |
+--------------------------------------+-----------------------------------+
| :ref:`ICE40_CONTROL <ICE40_CONTROL>` | :ref:`0xf000400c <ICE40_CONTROL>` |
+--------------------------------------+-----------------------------------+
| :ref:`ICE40_SS_N <ICE40_SS_N>`       | :ref:`0xf0004010 <ICE40_SS_N>`    |
+--------------------------------------+-----------------------------------+

ICE40_TX
^^^^^^^^

`Address: 0xf0004000 + 0x0 = 0xf0004000`

    Data to be sent via tx

    .. wavedrom::
        :caption: ICE40_TX

        {
            "reg": [
                {"name": "tx[7:0]", "bits": 8},
                {"bits": 24},
            ], "config": {"hspace": 400, "bits": 32, "lanes": 1 }, "options": {"hspace": 400, "bits": 32, "lanes": 1}
        }


ICE40_RX
^^^^^^^^

`Address: 0xf0004000 + 0x4 = 0xf0004004`

    Data that has been received via rx

    .. wavedrom::
        :caption: ICE40_RX

        {
            "reg": [
                {"name": "rx[7:0]", "bits": 8},
                {"bits": 24},
            ], "config": {"hspace": 400, "bits": 32, "lanes": 1 }, "options": {"hspace": 400, "bits": 32, "lanes": 1}
        }


ICE40_BUSY
^^^^^^^^^^

`Address: 0xf0004000 + 0x8 = 0xf0004008`

    Master Busy Signal

    .. wavedrom::
        :caption: ICE40_BUSY

        {
            "reg": [
                {"name": "busy", "bits": 1},
                {"bits": 31},
            ], "config": {"hspace": 400, "bits": 32, "lanes": 4 }, "options": {"hspace": 400, "bits": 32, "lanes": 4}
        }


ICE40_CONTROL
^^^^^^^^^^^^^

`Address: 0xf0004000 + 0xc = 0xf000400c`

    SPI Control.

    .. wavedrom::
        :caption: ICE40_CONTROL

        {
            "reg": [
                {"name": "enable",  "type": 4, "bits": 1},
                {"name": "cpol",  "bits": 1},
                {"name": "cpha",  "bits": 1},
                {"bits": 29}
            ], "config": {"hspace": 400, "bits": 32, "lanes": 4 }, "options": {"hspace": 400, "bits": 32, "lanes": 4}
        }


+-------+--------+------------------------+
| Field | Name   | Description            |
+=======+========+========================+
| [0]   | ENABLE | Initiate Communication |
+-------+--------+------------------------+
| [1]   | CPOL   | Clock Polarity Mode    |
+-------+--------+------------------------+
| [2]   | CPHA   | Clock Phase Mode       |
+-------+--------+------------------------+

ICE40_SS_N
^^^^^^^^^^

`Address: 0xf0004000 + 0x10 = 0xf0004010`

    Directly controls the Slave Select signal

    .. wavedrom::
        :caption: ICE40_SS_N

        {
            "reg": [
                {"name": "ss_n", "attr": 'reset: 1', "bits": 1},
                {"bits": 31},
            ], "config": {"hspace": 400, "bits": 32, "lanes": 4 }, "options": {"hspace": 400, "bits": 32, "lanes": 4}
        }


