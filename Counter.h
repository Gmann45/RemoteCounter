#ifndef __COUNTER_H__
#define __COUNTER_H__

class Counter {
	public:
		Counter(int count) {
			this->count = count;
		}

		int getCount(void) {
			return count;
		}

		void setCount(int count) {
			this->count = count;
		}

		void incCount(int count = 1) {
			this->count += count;
		}

		void decCount(int count = 1) {
			this->count -= count;
		}

	private:
		int count;
};


#endif
