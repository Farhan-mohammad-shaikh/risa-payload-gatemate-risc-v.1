GATEMATE_DEBUG_3
================

Register Listing for GATEMATE_DEBUG_3
-------------------------------------

+----------------------------------------------------+------------------------------------------+
| Register                                           | Address                                  |
+====================================================+==========================================+
| :ref:`GATEMATE_DEBUG_3_OE <GATEMATE_DEBUG_3_OE>`   | :ref:`0xf0001000 <GATEMATE_DEBUG_3_OE>`  |
+----------------------------------------------------+------------------------------------------+
| :ref:`GATEMATE_DEBUG_3_IN <GATEMATE_DEBUG_3_IN>`   | :ref:`0xf0001004 <GATEMATE_DEBUG_3_IN>`  |
+----------------------------------------------------+------------------------------------------+
| :ref:`GATEMATE_DEBUG_3_OUT <GATEMATE_DEBUG_3_OUT>` | :ref:`0xf0001008 <GATEMATE_DEBUG_3_OUT>` |
+----------------------------------------------------+------------------------------------------+

GATEMATE_DEBUG_3_OE
^^^^^^^^^^^^^^^^^^^

`Address: 0xf0001000 + 0x0 = 0xf0001000`

    GPIO Tristate(s) Control.

    .. wavedrom::
        :caption: GATEMATE_DEBUG_3_OE

        {
            "reg": [
                {"name": "oe", "bits": 1},
                {"bits": 31},
            ], "config": {"hspace": 400, "bits": 32, "lanes": 4 }, "options": {"hspace": 400, "bits": 32, "lanes": 4}
        }


GATEMATE_DEBUG_3_IN
^^^^^^^^^^^^^^^^^^^

`Address: 0xf0001000 + 0x4 = 0xf0001004`

    GPIO Input(s) Status.

    .. wavedrom::
        :caption: GATEMATE_DEBUG_3_IN

        {
            "reg": [
                {"name": "in", "bits": 1},
                {"bits": 31},
            ], "config": {"hspace": 400, "bits": 32, "lanes": 4 }, "options": {"hspace": 400, "bits": 32, "lanes": 4}
        }


GATEMATE_DEBUG_3_OUT
^^^^^^^^^^^^^^^^^^^^

`Address: 0xf0001000 + 0x8 = 0xf0001008`

    GPIO Ouptut(s) Control.

    .. wavedrom::
        :caption: GATEMATE_DEBUG_3_OUT

        {
            "reg": [
                {"name": "out", "bits": 1},
                {"bits": 31},
            ], "config": {"hspace": 400, "bits": 32, "lanes": 4 }, "options": {"hspace": 400, "bits": 32, "lanes": 4}
        }


