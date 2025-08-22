HYPERRAM
========

Register Listing for HYPERRAM
-----------------------------

+----------------------------------------------------+------------------------------------------+
| Register                                           | Address                                  |
+====================================================+==========================================+
| :ref:`HYPERRAM_CONFIG <HYPERRAM_CONFIG>`           | :ref:`0xf0002800 <HYPERRAM_CONFIG>`      |
+----------------------------------------------------+------------------------------------------+
| :ref:`HYPERRAM_STATUS <HYPERRAM_STATUS>`           | :ref:`0xf0002804 <HYPERRAM_STATUS>`      |
+----------------------------------------------------+------------------------------------------+
| :ref:`HYPERRAM_REG_CONTROL <HYPERRAM_REG_CONTROL>` | :ref:`0xf0002808 <HYPERRAM_REG_CONTROL>` |
+----------------------------------------------------+------------------------------------------+
| :ref:`HYPERRAM_REG_STATUS <HYPERRAM_REG_STATUS>`   | :ref:`0xf000280c <HYPERRAM_REG_STATUS>`  |
+----------------------------------------------------+------------------------------------------+
| :ref:`HYPERRAM_REG_WDATA <HYPERRAM_REG_WDATA>`     | :ref:`0xf0002810 <HYPERRAM_REG_WDATA>`   |
+----------------------------------------------------+------------------------------------------+
| :ref:`HYPERRAM_REG_RDATA <HYPERRAM_REG_RDATA>`     | :ref:`0xf0002814 <HYPERRAM_REG_RDATA>`   |
+----------------------------------------------------+------------------------------------------+

HYPERRAM_CONFIG
^^^^^^^^^^^^^^^

`Address: 0xf0002800 + 0x0 = 0xf0002800`


    .. wavedrom::
        :caption: HYPERRAM_CONFIG

        {
            "reg": [
                {"name": "rst",  "type": 4, "bits": 1},
                {"bits": 7},
                {"name": "latency",  "attr": '7', "bits": 8},
                {"bits": 16}
            ], "config": {"hspace": 400, "bits": 32, "lanes": 4 }, "options": {"hspace": 400, "bits": 32, "lanes": 4}
        }


+--------+---------+------------------------+
| Field  | Name    | Description            |
+========+=========+========================+
| [0]    | RST     | HyperRAM Rst.          |
+--------+---------+------------------------+
| [15:8] | LATENCY | HyperRAM Latency (X1). |
+--------+---------+------------------------+

HYPERRAM_STATUS
^^^^^^^^^^^^^^^

`Address: 0xf0002800 + 0x4 = 0xf0002804`


    .. wavedrom::
        :caption: HYPERRAM_STATUS

        {
            "reg": [
                {"name": "latency_mode",  "bits": 1},
                {"name": "clk_ratio",  "attr": '4', "bits": 4},
                {"bits": 27}
            ], "config": {"hspace": 400, "bits": 32, "lanes": 4 }, "options": {"hspace": 400, "bits": 32, "lanes": 4}
        }


+-------+--------------+---------------------------------------+
| Field | Name         | Description                           |
+=======+==============+=======================================+
| [0]   | LATENCY_MODE |                                       |
|       |              |                                       |
|       |              | +---------+-------------------+       |
|       |              | | Value   | Description       |       |
|       |              | +=========+===================+       |
|       |              | | ``0b0`` | Fixed Latency.    |       |
|       |              | +---------+-------------------+       |
|       |              | | ``0b1`` | Variable Latency. |       |
|       |              | +---------+-------------------+       |
+-------+--------------+---------------------------------------+
| [4:1] | CLK_RATIO    |                                       |
|       |              |                                       |
|       |              | +-------+---------------------------+ |
|       |              | | Value | Description               | |
|       |              | +=======+===========================+ |
|       |              | | ``4`` | HyperRAM Clk = Sys Clk/4. | |
|       |              | +-------+---------------------------+ |
|       |              | | ``2`` | HyperRAM Clk = Sys Clk/2. | |
|       |              | +-------+---------------------------+ |
+-------+--------------+---------------------------------------+

HYPERRAM_REG_CONTROL
^^^^^^^^^^^^^^^^^^^^

`Address: 0xf0002800 + 0x8 = 0xf0002808`


    .. wavedrom::
        :caption: HYPERRAM_REG_CONTROL

        {
            "reg": [
                {"name": "write",  "type": 4, "bits": 1},
                {"name": "read",  "type": 4, "bits": 1},
                {"bits": 6},
                {"name": "addr",  "bits": 2},
                {"bits": 22}
            ], "config": {"hspace": 400, "bits": 32, "lanes": 4 }, "options": {"hspace": 400, "bits": 32, "lanes": 4}
        }


+-------+-------+-------------------------------------------------------+
| Field | Name  | Description                                           |
+=======+=======+=======================================================+
| [0]   | WRITE | Issue Register Write.                                 |
+-------+-------+-------------------------------------------------------+
| [1]   | READ  | Issue Register Read.                                  |
+-------+-------+-------------------------------------------------------+
| [9:8] | ADDR  |                                                       |
|       |       |                                                       |
|       |       | +----------+----------------------------------------+ |
|       |       | | Value    | Description                            | |
|       |       | +==========+========================================+ |
|       |       | | ``0b00`` | Identification Register 0 (Read Only). | |
|       |       | +----------+----------------------------------------+ |
|       |       | | ``0b01`` | Identification Register 1 (Read Only). | |
|       |       | +----------+----------------------------------------+ |
|       |       | | ``0b10`` | Configuration Register 0.              | |
|       |       | +----------+----------------------------------------+ |
|       |       | | ``0b11`` | Configuration Register 1.              | |
|       |       | +----------+----------------------------------------+ |
+-------+-------+-------------------------------------------------------+

HYPERRAM_REG_STATUS
^^^^^^^^^^^^^^^^^^^

`Address: 0xf0002800 + 0xc = 0xf000280c`


    .. wavedrom::
        :caption: HYPERRAM_REG_STATUS

        {
            "reg": [
                {"name": "done",  "bits": 1},
                {"bits": 31}
            ], "config": {"hspace": 400, "bits": 32, "lanes": 4 }, "options": {"hspace": 400, "bits": 32, "lanes": 4}
        }


+-------+------+-----------------------+
| Field | Name | Description           |
+=======+======+=======================+
| [0]   | DONE | Register Access Done. |
+-------+------+-----------------------+

HYPERRAM_REG_WDATA
^^^^^^^^^^^^^^^^^^

`Address: 0xf0002800 + 0x10 = 0xf0002810`

    Register Write Data.

    .. wavedrom::
        :caption: HYPERRAM_REG_WDATA

        {
            "reg": [
                {"name": "reg_wdata[15:0]", "bits": 16},
                {"bits": 16},
            ], "config": {"hspace": 400, "bits": 32, "lanes": 1 }, "options": {"hspace": 400, "bits": 32, "lanes": 1}
        }


HYPERRAM_REG_RDATA
^^^^^^^^^^^^^^^^^^

`Address: 0xf0002800 + 0x14 = 0xf0002814`

    Register Read Data.

    .. wavedrom::
        :caption: HYPERRAM_REG_RDATA

        {
            "reg": [
                {"name": "reg_rdata[15:0]", "bits": 16},
                {"bits": 16},
            ], "config": {"hspace": 400, "bits": 32, "lanes": 1 }, "options": {"hspace": 400, "bits": 32, "lanes": 1}
        }


