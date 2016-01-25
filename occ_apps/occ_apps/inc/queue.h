#ifndef QUEUE_H
#define QUEUE_H



//void alarm_queue(uint8_t *src);
void occ_alram_process(uint8_t *src,uint8_t come_from);
void add_data_to_queue(unsigned char *src);
unsigned char get_data_from_queue(unsigned char *dst);

#endif


