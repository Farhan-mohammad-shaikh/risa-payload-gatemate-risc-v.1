GATEMATE_DEBUG_5
================

Register Listing for GATEMATE_DEBUG_5
-------------------------------------

+----------------------------------------------------+------------------------------------------+
| Register                                           | Address                                  |
+====================================================+==========================================+
| :ref:`GATEMATE_DEBUG_5_OE <GATEMATE_DEBUG_5_OE>`   | :ref:`0xf0002000 <GATEMATE_DEBUG_5_OE>`  |
+----------------------------------------------------+------------------------------------------+
| :ref:`GATEMATE_DEBUG_5_IN <GATEMATE_DEBUG_5_IN>`   | :ref:`0xf0002004 <GATEMATE_DEBUG_5_IN>`  |
+----------------------------------------------------+------------------------------------------+
| :ref:`GATEMATE_DEBUG_5_OUT <GATEMATE_DEBUG_5_OUT>` | :ref:`0xf0002008 <GATEMATE_DEBUG_5_OUT>` |
+----------------------------------------------------+------------------------------------------+

GATEMATE_DEBUG_5_OE
^^^^^^^^^^^^^^^^^^^

`Address: 0xf0002000 + 0x0 = 0xf0002000`

    GPIO Tristate(s) Control.

    .. wavedrom::
        :caption: GATEMATE_DEBUG_5_OE

        {
            "reg": [
                {"name": "oe", "bits": 1},
                {"bits": 31},
            ], "config": {"hspace": 400, "bits": 32, "lanes": 4 }, "options": {"hspace": 400, "bits": 32, "lanes": 4}
        }


GATEMATE_DEBUG_5_IN
^^^^^^^^^^^^^^^^^^^

`Address: 0xf0002000 + 0x4 = 0xf0002004`

    GPIO Input(s) Status.

    .. wavedrom::
        :caption: GATEMATE_DEBUG_5_IN

        {
            "reg": [
                {"name": "in", "bits": 1},
                {"bits": 31},
            ], "config": {"hspace": 400, "bits": 32, "lanes": 4 }, "options": {"hspace": 400, "bits": 32, "lanes": 4}
        }


GATEMATE_DEBUG_5_OUT
^^^^^^^^^^^^^^^^^^^^

`Address: 0xf0002000 + 0x8 = 0xf0002008`

    GPIO Ouptut(s) Control.

    .. wavedrom::
        :caption: GATEMATE_DEBUG_5_OUT

        {
            "reg": [
                {"name": "out", "bits": 1},
                {"bits": 31},
            ], "config": {"hspace": 400, "bits": 32, "lanes": 4 }, "options": {"hspace": 400, "bits": 32, "lanes": 4}
        }


