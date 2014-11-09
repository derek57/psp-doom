// additional stuff for PSP mp3 decoder implementation

int mp3_init(void);
void mp3_deinit(void);
int mp3_start_play(char *fname, int pos);

int mp3_job_started;

extern int mp3_volume;
int changeMp3Volume;
