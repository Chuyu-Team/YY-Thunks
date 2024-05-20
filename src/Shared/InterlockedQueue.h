#pragma once

namespace YY::Thunks::internal
{
    namespace
    {
        template<class Entry, size_t kMaxBlockSize>
        class InterlockedQueue
        {
        private:
            struct Block
            {
                size_t uLastReadIndex = 0;
                size_t uLastWriteIndex = 0;
                Block* pNextBlock = nullptr;
                Entry* arrLoopBuffer[kMaxBlockSize];

                bool IsEmpty()
                {
                    return uLastReadIndex == uLastWriteIndex;
                }

                bool IsFull()
                {
                    return uLastReadIndex + kMaxBlockSize == uLastWriteIndex;
                }
            };

            Block* pFirstReadBlock = nullptr;
            Block* pLastWriteBlock = nullptr;

        public:
            Entry* Pop() noexcept
            {
                if (!pFirstReadBlock)
                    return nullptr;

                for (;;)
                {
                    // 当前块任然有元素？
                    if (!pFirstReadBlock->IsEmpty())
                    {
                        auto _pTmp = pFirstReadBlock->arrLoopBuffer[pFirstReadBlock->uLastReadIndex % kMaxBlockSize];
                        pFirstReadBlock->uLastReadIndex += 1;
                        return _pTmp;
                    }

                    // 尝试流转到下一块
                    if (!pFirstReadBlock->pNextBlock)
                        return nullptr;

                    auto _pPendingDelete = pFirstReadBlock;
                    pFirstReadBlock = pFirstReadBlock->pNextBlock;
                    Delete(_pPendingDelete);
                }

                return nullptr;
            }

            void Push(_In_ Entry* _pEntry)
            {
                if (!pLastWriteBlock)
                {
                    pFirstReadBlock = pLastWriteBlock = New<Block>();
                }

                // 如果满了就尝试链接到下一块
                if (pLastWriteBlock->IsFull())
                {
                    auto _pNextBlock = New<Block>();
                    pLastWriteBlock->pNextBlock = _pNextBlock;
                    pLastWriteBlock = _pNextBlock;
                }

                pLastWriteBlock->arrLoopBuffer[pLastWriteBlock->uLastWriteIndex % kMaxBlockSize] = _pEntry;
                pLastWriteBlock->uLastWriteIndex += 1;
            }
        };
    }
}
