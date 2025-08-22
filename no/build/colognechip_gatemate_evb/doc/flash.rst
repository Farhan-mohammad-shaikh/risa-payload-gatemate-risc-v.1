FLASH
=====

Register Listing for FLASH
--------------------------

+--------------------------------------+-----------------------------------+
| Register                             | Address                           |
+======================================+===================================+
| :ref:`FLASH_TX <FLASH_TX>`           | :ref:`0xf0000800 <FLASH_TX>`      |
+--------------------------------------+-----------------------------------+
| :ref:`FLASH_RX <FLASH_RX>`           | :ref:`0xf0000804 <FLASH_RX>`      |
+--------------------------------------+-----------------------------------+
| :ref:`FLASH_BUSY <FLASH_BUSY>`       | :ref:`0xf0000808 <FLASH_BUSY>`    |
+--------------------------------------+-----------------------------------+
| :ref:`FLASH_CONTROL <FLASH_CONTROL>` | :ref:`0xf000080c <FLASH_CONTROL>` |
+--------------------------------------+-----------------------------------+
| :ref:`FLASH_SS_N <FLASH_SS_N>`       | :ref:`0xf0000810 <FLASH_SS_N>`    |
+--------------------------------------+-----------------------------------+

FLASH_TX
^^^^^^^^

`Address: 0xf0000800 + 0x0 = 0xf0000800`

    Data to be sent via tx

    .. wavedrom::
        :caption: FLASH_TX

        {
            "reg": [
                {"name": "tx[7:0]", "bits": 8},
                {"bits": 24},
            ], "config": {"hspace": 400, "bits": 32, "lanes": 1 }, "options": {"hspace": 400, "bits": 32, "lanes": 1}
        }


FLASH_RX
^^^^^^^^

`Address: 0xf0000800 + 0x4 = 0xf0000804`

    Data that has been received via rx

    .. wavedrom::
        :caption: FLASH_RX

        {
            "reg": [
                {"name": "rx[7:0]", "bits": 8},
                {"bits": 24},
            ], "config": {"hspace": 400, "bits": 32, "lanes": 1 }, "options": {"hspace": 400, "bits": 32, "lanes": 1}
        }


FLASH_BUSY
^^^^^^^^^^

`Address: 0xf0000800 + 0x8 = 0xf0000808`

    Master Busy Signal

    .. wavedrom::
        :caption: FLASH_BUSY

        {
            "reg": [
                {"name": "busy", "bits": 1},
                {"bits": 31},
            ], "config": {"hspace": 400, "bits": 32, "lanes": 4 }, "options": {"hspace": 400, "bits": 32, "lanes": 4}
        }


FLASH_CONTROL
^^^^^^^^^^^^^

`Address: 0xf0000800 + 0xc = 0xf000080c`

    SPI Control.

    .. wavedrom::
        :caption: FLASH_CONTROL

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

FLASH_SS_N
^^^^^^^^^^

`Address: 0xf0000800 + 0x10 = 0xf0000810`

    Directly controls the Slave Select signal

    .. wavedrom::
        :caption: FLASH_SS_N

        {
            "reg": [
                {"name": "ss_n", "attr": 'reset: 1', "bits": 1},
                {"bits": 31},
            ], "config": {"hspace": 400, "bits": 32, "lanes": 4 }, "options": {"hspace": 400, "bits": 32, "lanes": 4}
        }


