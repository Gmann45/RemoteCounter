#ifndef __COUNTER_H__
#define __COUNTER_H__

class Counter {
	public:
		Counter(int Count) : m_Count(Count) {

		}

		int getCount(void) {
			return m_Count;
		}

		void setCount(int Count) {
			m_Count = Count;
		}

		void incCount(void) {
			m_Count++;
		}

		void decCount(void) {
			m_Count--;
		}

	private:
		int m_Count;
};


#endif
