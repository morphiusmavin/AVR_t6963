test_write_data.c is used to test sending sample data to the AVR. It can also be used to test the sending/receiving<br />
of data to another linux box (with serial cables connected). One one box run 'test_data' with no params to read,<br />
and on the other, run 'test_data <iters> <starting rpm value> <starting value for all others><br />
- pic writes START_AVR in IDLE then goes to GET_REQ
- avr reads START_AVR then goes to REQ_DATA
- avr writes PIC24_GET_DATA then goes to WAIT_DATA_READY
- pic reads PIC24_GET_DATA then goes to GET_DATA
- pic writes PIC24_DATA_READY then goes to P24_WAIT_NEW_DATA
- avr reads PIC24_DATA_READY then goes to AVR_WAIT_NEW_DATA
- avr writes AVR_HAS_NEW_DATA then goes to SEND_NEW_DATA
- avr goes to AVR_IDLE
- pic reads AVR_HAS_NEW_DATA then goes to P24_STORE_NEW_DATA
- pic goes to IDLE
