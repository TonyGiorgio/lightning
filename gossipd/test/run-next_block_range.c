#include "../seeker.c"
#include <ccan/err/err.h>
#include <stdarg.h>
#include <stdio.h>

/* AUTOGENERATED MOCKS START */
/* Generated stub for new_reltimer_ */
struct oneshot *new_reltimer_(struct timers *timers UNNEEDED,
			      const tal_t *ctx UNNEEDED,
			      struct timerel expire UNNEEDED,
			      void (*cb)(void *) UNNEEDED, void *arg UNNEEDED)
{ fprintf(stderr, "new_reltimer_ called!\n"); abort(); }
/* Generated stub for query_channel_range */
bool query_channel_range(struct daemon *daemon UNNEEDED,
			 struct peer *peer UNNEEDED,
			 u32 first_blocknum UNNEEDED, u32 number_of_blocks UNNEEDED,
			 enum query_option_flags qflags UNNEEDED,
			 void (*cb)(struct peer *peer UNNEEDED,
				    u32 first_blocknum UNNEEDED, u32 number_of_blocks UNNEEDED,
				    const struct short_channel_id *scids UNNEEDED,
				    const struct channel_update_timestamps * UNNEEDED,
				    bool complete))
{ fprintf(stderr, "query_channel_range called!\n"); abort(); }
/* Generated stub for query_short_channel_ids */
bool query_short_channel_ids(struct daemon *daemon UNNEEDED,
			     struct peer *peer UNNEEDED,
			     const struct short_channel_id *scids UNNEEDED,
			     const u8 *query_flags UNNEEDED,
			     void (*cb)(struct peer *peer UNNEEDED, bool complete))
{ fprintf(stderr, "query_short_channel_ids called!\n"); abort(); }
/* Generated stub for queue_peer_msg */
void queue_peer_msg(struct peer *peer UNNEEDED, const u8 *msg TAKES UNNEEDED)
{ fprintf(stderr, "queue_peer_msg called!\n"); abort(); }
/* Generated stub for random_peer */
struct peer *random_peer(struct daemon *daemon UNNEEDED,
			 bool (*check_peer)(const struct peer *peer))
{ fprintf(stderr, "random_peer called!\n"); abort(); }
/* Generated stub for status_failed */
void status_failed(enum status_failreason code UNNEEDED,
		   const char *fmt UNNEEDED, ...)
{ fprintf(stderr, "status_failed called!\n"); abort(); }
/* Generated stub for status_fmt */
void status_fmt(enum log_level level UNNEEDED, const char *fmt UNNEEDED, ...)

{ fprintf(stderr, "status_fmt called!\n"); abort(); }
/* Generated stub for would_ratelimit_cupdate */
bool would_ratelimit_cupdate(struct routing_state *rstate UNNEEDED,
			     const struct half_chan *hc UNNEEDED,
			     u32 timestamp UNNEEDED)
{ fprintf(stderr, "would_ratelimit_cupdate called!\n"); abort(); }
/* AUTOGENERATED MOCKS END */

static void test_block_range(struct seeker *seeker,
			     u32 blockheight,
			     u32 first, u32 last,
			     ...)
{
	int start, end, num;
	u32 first_blocknum, number_of_blocks;
	va_list ap;

	seeker->daemon->current_blockheight = blockheight;
	seeker->scid_probe_start = first;
	seeker->scid_probe_end = last;

	num = last - first + 1;
	va_start(ap, last);
	while ((start = va_arg(ap, int)) != -1) {
		end = va_arg(ap, int);
		if (!next_block_range(seeker, num,
				      &first_blocknum, &number_of_blocks))
			abort();
		if (first_blocknum != start
		    || number_of_blocks != end - start + 1) {
			errx(1, "Expected %u-%u but got %u-%u",
			     start, end,
			     first_blocknum, first_blocknum+number_of_blocks-1);
		}
		num = end - start + 1;
	}
	if (next_block_range(seeker, num, &first_blocknum, &number_of_blocks))
		abort();
	va_end(ap);
}

int main(void)
{
	struct seeker *seeker = tal(NULL, struct seeker);

	setup_locale();

	seeker->daemon = tal(seeker, struct daemon);

	/* Case where we start at beginning */
	test_block_range(seeker, 100,
			 0, 0,
			 1, 2,
			 3, 6,
			 7, 14,
			 15, 30,
			 31, 62,
			 63, 100,
			 -1);

	/* Case where we start at end */
	test_block_range(seeker, 100,
			 100, 100,
			 98, 99,
			 94, 97,
			 86, 93,
			 70, 85,
			 38, 69,
			 0, 37,
			 -1);

	/* Start in the middle. */
	test_block_range(seeker, 100,
			 50, 59,
			 30, 49,
			 0, 29,
			 60, 100,
			 -1);

	tal_free(seeker);
}
