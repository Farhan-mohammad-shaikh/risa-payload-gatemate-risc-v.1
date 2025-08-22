UART_LOGGING
============

Register Listing for UART_LOGGING
---------------------------------

+----------------------------------------------------------+---------------------------------------------+
| Register                                                 | Address                                     |
+==========================================================+=============================================+
| :ref:`UART_LOGGING_TX_DATA <UART_LOGGING_TX_DATA>`       | :ref:`0xf000f800 <UART_LOGGING_TX_DATA>`    |
+----------------------------------------------------------+---------------------------------------------+
| :ref:`UART_LOGGING_RX_DATA <UART_LOGGING_RX_DATA>`       | :ref:`0xf000f804 <UART_LOGGING_RX_DATA>`    |
+----------------------------------------------------------+---------------------------------------------+
| :ref:`UART_LOGGING_CONTROL <UART_LOGGING_CONTROL>`       | :ref:`0xf000f808 <UART_LOGGING_CONTROL>`    |
+----------------------------------------------------------+---------------------------------------------+
| :ref:`UART_LOGGING_STATUS <UART_LOGGING_STATUS>`         | :ref:`0xf000f80c <UART_LOGGING_STATUS>`     |
+----------------------------------------------------------+---------------------------------------------+
| :ref:`UART_LOGGING_EV_STATUS <UART_LOGGING_EV_STATUS>`   | :ref:`0xf000f810 <UART_LOGGING_EV_STATUS>`  |
+----------------------------------------------------------+---------------------------------------------+
| :ref:`UART_LOGGING_EV_PENDING <UART_LOGGING_EV_PENDING>` | :ref:`0xf000f814 <UART_LOGGING_EV_PENDING>` |
+----------------------------------------------------------+---------------------------------------------+
| :ref:`UART_LOGGING_EV_ENABLE <UART_LOGGING_EV_ENABLE>`   | :ref:`0xf000f818 <UART_LOGGING_EV_ENABLE>`  |
+----------------------------------------------------------+---------------------------------------------+

UART_LOGGING_TX_DATA
^^^^^^^^^^^^^^^^^^^^

`Address: 0xf000f800 + 0x0 = 0xf000f800`

    input data to be transmitted over UART

    .. wavedrom::
        :caption: UART_LOGGING_TX_DATA

        {
            "reg": [
                {"name": "tx_data[7:0]", "bits": 8},
                {"bits": 24},
            ], "config": {"hspace": 400, "bits": 32, "lanes": 1 }, "options": {"hspace": 400, "bits": 32, "lanes": 1}
        }


UART_LOGGING_RX_DATA
^^^^^^^^^^^^^^^^^^^^

`Address: 0xf000f800 + 0x4 = 0xf000f804`

    output data received via UART

    .. wavedrom::
        :caption: UART_LOGGING_RX_DATA

        {
            "reg": [
                {"name": "rx_data[7:0]", "bits": 8},
                {"bits": 24},
            ], "config": {"hspace": 400, "bits": 32, "lanes": 1 }, "options": {"hspace": 400, "bits": 32, "lanes": 1}
        }


UART_LOGGING_CONTROL
^^^^^^^^^^^^^^^^^^^^

`Address: 0xf000f800 + 0x8 = 0xf000f808`

    UART Control.

    .. wavedrom::
        :caption: UART_LOGGING_CONTROL

        {
            "reg": [
                {"name": "DIN_VLD",  "type": 4, "bits": 1},
                {"bits": 31}
            ], "config": {"hspace": 400, "bits": 32, "lanes": 4 }, "options": {"hspace": 400, "bits": 32, "lanes": 4}
        }


+-------+---------+-----------------------------------------------------------------------+
| Field | Name    | Description                                                           |
+=======+=========+=======================================================================+
| [0]   | DIN_VLD | input data (tx_data) are valid, this bit will be cleared automaticaly |
+-------+---------+-----------------------------------------------------------------------+

UART_LOGGING_STATUS
^^^^^^^^^^^^^^^^^^^

`Address: 0xf000f800 + 0xc = 0xf000f80c`

    UART Status.

    .. wavedrom::
        :caption: UART_LOGGING_STATUS

        {
            "reg": [
                {"name": "DIN_RDY",  "bits": 1},
                {"name": "DOUT_VLD",  "bits": 1},
                {"name": "FE",  "bits": 1},
                {"name": "PE",  "bits": 1},
                {"bits": 28}
            ], "config": {"hspace": 400, "bits": 32, "lanes": 4 }, "options": {"hspace": 400, "bits": 32, "lanes": 4}
        }


+-------+----------+----------------------------------------------------------------------------+
| Field | Name     | Description                                                                |
+=======+==========+============================================================================+
| [0]   | DIN_RDY  | transmitter is ready and valid input data will be accepted for transmiting |
+-------+----------+----------------------------------------------------------------------------+
| [1]   | DOUT_VLD | output data (DOUT) are valid (is assert only for one clock cycle)          |
+-------+----------+----------------------------------------------------------------------------+
| [2]   | FE       | Frame_Error stop bit was invalid (is assert only for one clock cycle)      |
+-------+----------+----------------------------------------------------------------------------+
| [3]   | PE       | Parity_Error parity bit was invalid (is assert only for one clock cycle)   |
+-------+----------+----------------------------------------------------------------------------+

UART_LOGGING_EV_STATUS
^^^^^^^^^^^^^^^^^^^^^^

`Address: 0xf000f800 + 0x10 = 0xf000f810`

    This register contains the current raw level of the rx event trigger.  Writes to
    this register have no effect.

    .. wavedrom::
        :caption: UART_LOGGING_EV_STATUS

        {
            "reg": [
                {"name": "rx",  "bits": 1},
                {"bits": 31}
            ], "config": {"hspace": 400, "bits": 32, "lanes": 4 }, "options": {"hspace": 400, "bits": 32, "lanes": 4}
        }


+-------+------+---------------------------+
| Field | Name | Description               |
+=======+======+===========================+
| [0]   | RX   | Level of the ``rx`` event |
+-------+------+---------------------------+

UART_LOGGING_EV_PENDING
^^^^^^^^^^^^^^^^^^^^^^^

`Address: 0xf000f800 + 0x14 = 0xf000f814`

    When a  rx event occurs, the corresponding bit will be set in this register.  To
    clear the Event, set the corresponding bit in this register.

    .. wavedrom::
        :caption: UART_LOGGING_EV_PENDING

        {
            "reg": [
                {"name": "rx",  "bits": 1},
                {"bits": 31}
            ], "config": {"hspace": 400, "bits": 32, "lanes": 4 }, "options": {"hspace": 400, "bits": 32, "lanes": 4}
        }


+-------+------+------------------------------------------------------------------------------+
| Field | Name | Description                                                                  |
+=======+======+==============================================================================+
| [0]   | RX   | `1` if a `rx` event occurred. This Event is triggered on a **falling** edge. |
+-------+------+------------------------------------------------------------------------------+

UART_LOGGING_EV_ENABLE
^^^^^^^^^^^^^^^^^^^^^^

`Address: 0xf000f800 + 0x18 = 0xf000f818`

    This register enables the corresponding rx events.  Write a ``0`` to this
    register to disable individual events.

    .. wavedrom::
        :caption: UART_LOGGING_EV_ENABLE

        {
            "reg": [
                {"name": "rx",  "bits": 1},
                {"bits": 31}
            ], "config": {"hspace": 400, "bits": 32, "lanes": 4 }, "options": {"hspace": 400, "bits": 32, "lanes": 4}
        }


+-------+------+------------------------------------------+
| Field | Name | Description                              |
+=======+======+==========================================+
| [0]   | RX   | Write a ``1`` to enable the ``rx`` Event |
+-------+------+------------------------------------------+

