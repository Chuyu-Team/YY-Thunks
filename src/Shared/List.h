#pragma once

namespace YY::Thunks
{
    namespace
    {
        template<typename ListEntry>
        struct ListEntryImpl
        {
            ListEntry* pPrior = nullptr;
            ListEntry* pNext = nullptr;
        };

        template<typename ListEntry>
        class ListImpl
        {
        public:
            ListEntry* pFirst = nullptr;
            ListEntry* pLast = nullptr;

            void __fastcall PushBack(ListEntry* _pItem) noexcept
            {
                if (!_pItem)
                    return;

                _pItem->pNext = nullptr;
                _pItem->pPrior = pLast;
                if (pLast)
                {
                    pLast->pNext = _pItem;
                }
                pLast = _pItem;

                if (!pFirst)
                    pFirst = _pItem;
            }

            bool __fastcall Remove(ListEntry* _pItem) noexcept
            {
                if (!_pItem)
                    return false;

                if (!pFirst)
                    return false;

                auto _pPrior = _pItem->pPrior;
                auto _pNext = _pItem->pNext;
                if (_pPrior)
                {
                    _pPrior->pNext = _pNext;
                }

                if (_pNext)
                {
                    _pNext->pPrior = _pPrior;
                }

                if (pFirst == _pItem)
                {
                    pFirst = _pNext;
                }

                if (pLast == _pItem)
                {
                    pLast = _pPrior;
                }

                return true;
            }
        };
    }
}
