// ilg_file_demo.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "pch.h"
#include <Windows.h>
#include <Ole2.h>
#include <stdio.h>
#include <tchar.h>

void read_storage(LPSTORAGE pstg)
{
	LPSTORAGE psubstg = NULL;
	LPSTREAM pstream = NULL;
	LPENUMSTATSTG penum = NULL;
	STATSTG statstg;
	HRESULT hr = S_OK;

	if (pstg->EnumElements(0, NULL, 0, &penum) != NOERROR)
	{
		return;
	}

	while (penum->Next(1, &statstg, NULL) == NOERROR)
	{
		if (statstg.type == STGTY_STORAGE)
		{
			hr = pstg->OpenStorage(statstg.pwcsName,
				NULL,
				STGM_READWRITE | STGM_SHARE_EXCLUSIVE,
				NULL,
				0,
				&psubstg);
			if (hr == S_OK)
			{
				read_storage(psubstg);
				psubstg->Release();
			}
		}
		else if (statstg.type == STGTY_STREAM)
		{
			hr = pstg->OpenStream(statstg.pwcsName,
				NULL,
				STGM_READWRITE | STGM_SHARE_EXCLUSIVE,
				0,
				&pstream);
			if (hr == S_OK)
			{
				// Let InstallShield produce this error: "Error extracting support
				// files: Catastrophic failure"
				if (_tcsicmp(L"StrIndex_hkStream", statstg.pwcsName) == 0 ||
					_tcsicmp(L"ID_6_SEQ_PROPERTIES", statstg.pwcsName) == 0 ||
					_tcsicmp(L"ID_5_SEQ_PROPERTIES", statstg.pwcsName) == 0 ||
					_tcsicmp(L"ID_3_SEQ_PROPERTIES", statstg.pwcsName) == 0 ||
					_tcsicmp(L"ID_2_SEQ_PROPERTIES", statstg.pwcsName) == 0 ||
					_tcsicmp(L"ID_1_SEQ_PROPERTIES", statstg.pwcsName) == 0 ||
					_tcsicmp(L"FEATURELOG_STDPROPS", statstg.pwcsName) == 0
					)
				{
					printf("%S size: %lld\n", statstg.pwcsName,
						statstg.cbSize.QuadPart);

					ULARGE_INTEGER ui = { 0 };
					pstream->SetSize(ui);
					hr = pstream->Commit(statstg.grfStateBits);
				}
			}

			pstream->Release();
		}

		CoTaskMemFree(statstg.pwcsName);
	}

	penum->Release();
}

int main()
{
	IStorage *pstg = NULL;
	HRESULT hr = S_OK;
	TCHAR file_name[] = L"setup.ilg";

	hr = StgOpenStorageEx(file_name,
		STGM_READWRITE | STGM_SHARE_DENY_WRITE,
		STGFMT_ANY,
		0,
		NULL,
		NULL,
		IID_IStorage,
		reinterpret_cast<void **>(&pstg));
	if (FAILED(hr))
	{
		printf("Couldn't open storage \"%S\" (hr = %08x)\n",
			file_name, hr);
	}
	else
	{
		read_storage(pstg);
		pstg->Release();
	}

	return 0;
}
