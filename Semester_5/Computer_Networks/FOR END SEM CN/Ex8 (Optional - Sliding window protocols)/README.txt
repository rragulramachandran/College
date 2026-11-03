Sliding Window Protocols over UDP sockets
=========================================
Each protocol has a sender and a receiver (run the receiver first, in a separate terminal).
The word you enter is split into one-character frames.
The sender asks for a frame number to "lose" once, so you can see the retransmission.
Use 0 for normal transmission. Timeout = 2 seconds. Port = 9000 (localhost).

  stop_and_wait_sender.c / stop_and_wait_receiver.c
  go_back_n_sender.c     / go_back_n_receiver.c
  selective_repeat_sender.c / selective_repeat_receiver.c

Run (example for Go-Back-N):
  gcc go_back_n_receiver.c -o rec && ./rec        (terminal 1)
  gcc go_back_n_sender.c   -o send && ./send      (terminal 2)

Difference at a glance:
  Stop-and-Wait : window = 1, wait for ACK of every frame.
  Go-Back-N     : window = 4, cumulative ACK, receiver discards out-of-order frames,
                  on timeout sender resends ALL frames from the lost one onward.
  Selective Rpt : window = 4, individual ACKs, receiver buffers out-of-order frames,
                  on timeout sender resends ONLY the lost frame.
