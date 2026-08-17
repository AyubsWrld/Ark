# Audio Capture Abstraction

So as to separate how we capture audio from the rest of the pipelinme we use an Abstraction (`FAudioDevice`) over an arbitrary hardware device capable of the following: 
- Initializing.
- Pausing. 
- Resuming. 
- Clearing (captured audio). 
- (optional) Callback
- Get. (to get audio data from a circular buffer).
- A Length of milliseconds.
- A sample rate. 
- Whether or not it is running 
- A mutex 
- Captured audio samples
- Two pointers into the circular buffer.


