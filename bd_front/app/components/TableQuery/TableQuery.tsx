import { Table, TableHeader, TableColumn, TableBody, TableRow, TableCell, Pagination, getKeyValue } from "@nextui-org/react";
import { useState, useMemo } from "react";
import { AppleRecord, AppleRecordKeys } from "@/app/types/appleRecord";


export default function TableQuery({ data, headers }: { data: AppleRecord[], headers: AppleRecordKeys[] }) {

    const [page, setPage] = useState(1);
    const rowsPerPage = 10;

    const pages = Math.ceil(data.length / rowsPerPage);

    const items = useMemo(() => {
        const start = (page - 1) * rowsPerPage;
        const end = start + rowsPerPage;

        return data.slice(start, end);
    }, [page, data]);

    return (
        <Table isStriped
            aria-label="Table of Apple Store data"

            bottomContent={
                <div className="flex w-full justify-center">
                    <Pagination
                        isCompact
                        showControls
                        showShadow
                        color="secondary"
                        page={page}
                        total={pages}
                        onChange={(page: number) => setPage(page)}
                    />
                </div>
            }
            classNames={{
                wrapper: "min-h-[222px]",
            }}
        >
            <TableHeader>
                {headers.map((header) => (
                    <TableColumn key={header}>{header}</TableColumn>
                ))}
            </TableHeader>
            <TableBody items={items} emptyContent={"No rows to display here :("}>
                {(item: any) => (
                    <TableRow key={item.name}>
                        {(columnKey: any) => <TableCell>{getKeyValue(item, columnKey)}</TableCell>}
                    </TableRow>
                )}
            </TableBody>
        </Table>
    );
}