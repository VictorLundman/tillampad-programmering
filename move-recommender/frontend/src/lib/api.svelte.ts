import { PUBLIC_API_URL } from "$env/static/public";
import { writable, get } from "svelte/store";

export const userJwt = writable<string | null>(null);

export async function fetchApi<T>(
  path: `/${string}`,
  options: RequestInit = {}
): Promise<
  | {
      success: false;
      status: number;
      message: string;
    }
  | {
      success: true;
      status: number;
      response: T;
    }
> {
  const url = `${PUBLIC_API_URL}${path}`;
  const headers = new Headers(options.headers || {});
  headers.set("Accept", "application/json");
  headers.set(
    "Content-Type",
    headers.get("Content-Type") || "application/json"
  );
  if (get(userJwt)) {
    headers.set("Authorization", `Bearer ${get(userJwt)}`);
  }
  const response = await fetch(url, { ...options, headers });
  if (!response.ok) {
    return {
      success: false,
      status: response.status,
      message: response.statusText,
    };
  }
  return {
    success: true,
    status: response.status,
    response: await response.json(),
  };
}
