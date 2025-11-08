#ifndef PMM_H
#define PMM_H

void pmm_init(void);
void* alloc_page(void);

void free_page(void* page);
int pmm_free_pages(void);

#endif // PMM_H
